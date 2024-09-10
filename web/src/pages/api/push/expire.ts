import type { APIRoute } from 'astro';

import { db } from "../../../utils/db";
import { getCurrentUser } from '../../../utils/auth';

export const POST: APIRoute = async ({cookies, request}) => {
    const IDpush = await request.json();
    const user = getCurrentUser(cookies);

    if (!user) {
        return new Response(null, { status: 401 });
    }
    
    if (!IDpush || typeof IDpush !== 'number' || IDpush <= 0) {
        return new Response(null, { status: 400 });
    }

    await db.request()
        .input('IDpush', IDpush)
        .execute('sp_push_expire');

    return new Response(null, { status: 200 });
}
