import 'dotenv/config';

import { logWith } from './util/log.js';
import { db } from './db/db.js';

import express from 'express';
const app = express();

import bodyparser from 'body-parser';
app.use(bodyparser.json());
app.use((req, res, next) => {
    res.setHeader('Content-Type', 'application/json');
    next();
});

app.post('/api/mailbox/config', async (req, res) => {
    // todo implement
    console.log(req.body);
    res.json({ from: 54, to: 58 });
});

app.post('/api/login', async (req, res) => {
    const result = await db.request()
        .input('username', req.body.username)
        .input('password', req.body.password)
        .execute('sp_user_login');

    res.json(result.recordset[0]?.result?? 0);
});

app.post('/api/register', async (req, res) => {
    console.log(req.body);
    const result = await db.request()
        .input('IDuser', req.body.IDuser)
        .input('public_code', req.body.public_code)
        .execute('sp_register');

    res.json(result.recordset[0]?.result?? 0);
});

app.listen(process.env.PORT || 6245, () => logWith('Express', 'Server started'));
