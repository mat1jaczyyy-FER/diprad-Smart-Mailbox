import 'dotenv/config';

import { logWith } from './util/log.js';
import { db } from './db/db.js';

import express from 'express';
const app = express();

import bodyparser from 'body-parser';
app.use(bodyparser.json());

type UltrasoundConfig = [number, number];
type InfraredConfig = [number, number][];

// todo handle errors (they currently take the node process down in production)

app.post('/internal/mailbox/config', async (req, res) => {
    const result = await db.request()
        .input('private_code', req.body.private_code)
        .execute('sp_mailbox_config');

    const type = result.recordset[0]?.IDtype;

    if (!type) {
        res.sendStatus(401);
        return;
    }

    let buffer: Buffer = Buffer.alloc(0);

    if (type == 1) {
        const config: UltrasoundConfig = JSON.parse(result.recordset[0].config);

        buffer = Buffer.alloc(config.length * 4);

        for (let i = 0; i < config.length; i++) {
            buffer.writeUInt32LE(config[i], i * 4);
        }

    } else if (type == 2) {
        const config: InfraredConfig = JSON.parse(result.recordset[0].config);

        buffer = Buffer.alloc(config.length * 8 + 4);

        buffer.writeUInt32LE(config.length, 0);

        for (let i = 0; i < config.length; i++) {
            buffer.writeUInt32LE(config[i][0], i * 8 + 4);
            buffer.writeUInt32LE(config[i][1], i * 8 + 8);
        }
    }

    res.setHeader('Content-Type', 'application/octet-stream');
    res.setHeader('Content-Transfer-Encoding', 'base64');

    res.send(buffer.toString('base64'));
});

app.post('/internal/mailbox/battery', async (req, res) => {
    await db.request()
        .input('private_code', req.body.private_code)
        .input('low', req.body.low)
        .execute('sp_mailbox_battery');
    
    res.sendStatus(200);
});

app.post('/internal/login', async (req, res) => {
    const result = await db.request()
        .input('username', req.body.username)
        .input('password', req.body.password)
        .execute('sp_user_login');

    res.setHeader('Content-Type', 'application/json');
    res.json(result.recordset[0]?.result?? 0);
});

app.post('/internal/register', async (req, res) => {
    const result = await db.request()
        .input('IDuser', req.body.IDuser)
        .input('public_code', req.body.public_code)
        .execute('sp_register');

    res.setHeader('Content-Type', 'application/json');
    res.json(result.recordset[0]?.result?? 0);
});

app.listen(process.env.PORT || 6246, () => logWith('Express', 'Server started'));
