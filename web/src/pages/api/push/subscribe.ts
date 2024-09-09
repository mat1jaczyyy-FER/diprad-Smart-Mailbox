import type { APIRoute } from 'astro';

import { db } from "../../../utils/db";
import { getCurrentUser } from '../../../utils/auth';

export const POST: APIRoute = async ({cookies, request}) => {
    const req = await request.json();
    const user = getCurrentUser(cookies);

    if (!user) {
        return new Response(null, { status: 401 });
    }
    
    if (!req.subscription || !req.subscription.endpoint) {
        return new Response(null, { status: 400 });
    }

    const result = await db.request()
        .input('IDuser', user.id)
        .input('public_code', req.public_code)
        .input('subscription', JSON.stringify(req.subscription))
        .execute('sp_push_subscribe');

    const success: number = result.recordset[0]?.result?? 0;
    if (!success) {
        return new Response(null, { status: 406 });
    }

    return new Response(success.toString());
}
