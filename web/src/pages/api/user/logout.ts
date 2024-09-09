import type { APIRoute } from 'astro';
import env from "../../../utils/env";

export const POST: APIRoute = async ({ cookies }) => {
    cookies.set(env.cookieName, "", { maxAge: 0, path: '/' });
    return new Response(null, { status: 200 });
};

export const GET = POST;
