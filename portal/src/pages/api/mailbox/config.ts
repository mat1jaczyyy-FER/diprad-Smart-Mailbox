import type { APIRoute } from 'astro';

export const POST: APIRoute = async ({request}) => {
    const req = await request.json();
    console.log(req);
    // todo database call
    return new Response(JSON.stringify({ from: 54, to: 58 }));
}
