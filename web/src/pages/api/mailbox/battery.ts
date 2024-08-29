import type { APIRoute } from "astro";
import { db } from "../../../utils/db";

export const POST: APIRoute = async ({ request }) => {
    const req = await request.json();

    await db.request()
        .input('private_code', req.private_code)
        .input('low', req.low)
        .execute('sp_mailbox_battery');
    
    return new Response(null, { status: 200 });
};
