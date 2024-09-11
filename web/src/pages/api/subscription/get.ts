import type { APIRoute } from 'astro';

import { db } from "../../../utils/db";
import { getCurrentUser } from '../../../utils/auth';

export const POST: APIRoute = async ({cookies, request}) => {
    const req = await request.json();
    const user = getCurrentUser(cookies);

    if (!user) {
        return new Response(null, { status: 401 });
    }

    const result = await db.request()
        .input('IDuser', user.id)
        .input('IDpush', req.IDpush)
        .execute('sp_subscription_get');

    return new Response(JSON.stringify(result.recordset.map(i => i.public_code)), { status: 200 });
}
