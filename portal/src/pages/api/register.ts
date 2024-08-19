import type { APIRoute } from 'astro';

import env from "../../utils/env";
import { verifyToken } from '../../utils/auth';
import { serverAPI } from '../../utils/server';

export const POST: APIRoute = async ({cookies, request}) => {
    const req = await request.json();
    const cookie = cookies.get(env.cookieName)?.value;
    const user = verifyToken(cookie);

    if (!user) {
        return new Response(null, { status: 401 });
    }

    const success: number = await serverAPI("api/register", {
        IDuser: user.id,
        public_code: req.public_code
    });
    
    if (!success) {
        return new Response(null, { status: 406 });
    }

    return new Response(success.toString());
}
