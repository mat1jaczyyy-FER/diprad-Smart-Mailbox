import type { APIRoute } from 'astro';

import env from "../../utils/env";
import { createToken } from "../../utils/token";

export const POST: APIRoute = async ({request}) => {
    const req = await request.json();
    const token = createToken(req.username, req.password, env.maxAge);

    return token
        ? new Response(token)
        : new Response(null, { status: 401 });
}
