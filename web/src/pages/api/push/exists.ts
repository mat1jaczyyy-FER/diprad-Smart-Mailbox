import type { APIRoute } from 'astro';

import { db } from "../../../utils/db";
import { getCurrentUser } from '../../../utils/auth';

export const POST: APIRoute = async ({cookies, request}) => {
    const req = await request.json();
    const user = getCurrentUser(cookies);

    if (!user) {
        return new Response(null, { status: 401 });
    }
    
    if (!req) {
        return new Response(null, { status: 400 });
    }

    const result = await db.request()
        .input('subscription', JSON.stringify(req))
        .execute('sp_push_exists');

    const success: number = result.recordset[0]?.result?? 0;

    return new Response(JSON.stringify(!!success));
}
