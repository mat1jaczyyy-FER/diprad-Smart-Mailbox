import type { APIRoute } from "astro";

import { db } from "../../../utils/db";
import webpush from "../../../utils/webpush";

export const POST: APIRoute = async ({ request }) => {
    const req = await request.json();

    const result = await db.request()
        .input('private_code', req.private_code)
        .execute('sp_mailbox_notify');

    try {
        for (const i of result.recordset) {
            webpush.sendNotification(
                JSON.parse(i.subscription),
                JSON.stringify({
                    public_code: i.public_code,
                    last_push: i.last_push
                })
            ).catch(async (err: webpush.WebPushError) => {
                if (err.statusCode === 404 || err.statusCode === 410) {
                    await db.request()
                        .input('IDpush', i.IDpush)
                        .execute('sp_push_expire');
                } else {
                    throw err;
                }
            });
        }

    } catch (e) {
        console.error("push notification error:", e);

    } finally {
        return new Response(null, { status: 200 });
    }
};
