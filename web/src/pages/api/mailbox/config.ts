import type { APIRoute } from "astro";
import { db } from "../../../utils/db";

type UltrasonicConfig = [number, number];
type InfraredConfig = [number, number][];

export const POST: APIRoute = async ({ request }) => {
    const req = await request.json();

    const result = await db.request()
        .input('private_code', req.private_code)
        .execute('sp_mailbox_config');

    const type = result.recordset[0]?.IDtype;

    if (!type) {
        return new Response(null, { status: 401 });
    }

    let buffer: Buffer = Buffer.alloc(0);

    if (type == 1) {
        const config: UltrasonicConfig = JSON.parse(result.recordset[0].config);

        buffer = Buffer.alloc(4 + config.length * 4);
        
        buffer.writeUInt32LE((new Date().getTime() / 1000) % 86400, 0);

        for (let i = 0; i < config.length; i++) {
            buffer.writeUInt32LE(config[i], 4 + i * 4);
        }

    } else if (type == 2) {
        const config: InfraredConfig = JSON.parse(result.recordset[0].config);

        buffer = Buffer.alloc(4 + 4 + config.length * 8);
        
        buffer.writeUInt32LE((new Date().getTime() / 1000) % 86400, 0);

        buffer.writeUInt32LE(config.length, 4);

        for (let i = 0; i < config.length; i++) {
            buffer.writeUInt32LE(config[i][0], 2 * 4 + i * 8);
            buffer.writeUInt32LE(config[i][1], 2 * 4 + i * 8 + 4);
        }
    }

    return new Response(
        buffer.toString('base64'),
        {
            headers: {
                'Content-Type': 'application/octet-stream',
                'Content-Transfer-Encoding': 'base64'
            }
        }
    );
};
