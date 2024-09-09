import type { APIRoute } from 'astro';
import env from "../../../utils/env";
import { tryCreateToken } from "../../../utils/auth";

export const POST: APIRoute = async ({ cookies, request }) => {
    const req = await request.json();
    const token = await tryCreateToken(req.username, req.password, env.maxAge);

    if (token)
        cookies.set(env.cookieName, token, { maxAge: env.maxAge, path: '/' });

    return new Response(null, { status: token? 200 : 401 });
};