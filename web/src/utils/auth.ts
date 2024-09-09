import util from 'util';
import crypto from 'crypto';
const scryptAsync: (digest: Buffer, salt: Buffer, keylen: number) => Promise<Buffer> = util.promisify(crypto.scrypt);

import type { AstroCookies } from 'astro';
import jwt from "jsonwebtoken";

import { db } from "./db";
import env from "./env";
import type { Token } from "./types";

function signToken(username: string, id: number, maxAge: number) {
    return jwt.sign(
        {
            expires: Date.now() + maxAge * 1000,
            id: id,
            username: username
        } as Token,
        env.secret
    );
}

function verifyToken(token?: string) {
    switch (token) {
        case "undefined":
        case undefined:
        case "null":
        case null:
            return null;
    }

    const data = jwt.verify(token, env.secret) as Token;
    if (!data.username)
        return null;

    const untilExpiry = (data.expires - Date.now()) / 1000;
    if (untilExpiry < 0)
        return null;

    return data;
}

async function hashPassword(password: string, salt: Buffer) {
    const hmac = crypto.createHmac('sha256', env.secret);
    hmac.update(password);
    return await scryptAsync(hmac.digest(), salt, 64);
}

export async function tryCreateToken(username: string, password: string, maxAge: number) {
    const salt = crypto.randomBytes(16);
    const hash = await hashPassword(password, salt);

    const result = await db.request()
        .input('username', username)
        .input('salt', salt)
        .input('hash', hash)
        .execute('sp_user_register');
    
    const id: number = result.recordset[0]?.result?? 0;

    if (!id)
        return null;

    return signToken(username, id, maxAge);
}

export async function tryGetToken(username: string, password: string, maxAge: number) {
    const result = await db.request()
        .input('username', username)
        .execute('sp_user_login');

    if (!result.recordset[0]?.ID)
        return null;

    const { ID: id, salt, hash: hashdb } = result.recordset[0]?? {};

    if (!id)
        return null;
    
    const hash = await hashPassword(password, salt);

    if (Buffer.compare(hash, hashdb) != 0)
        return null;

    return signToken(username, id, maxAge);
}

export function getCurrentUser(cookies: AstroCookies) {
    const cookie = cookies.get(env.cookieName)?.value;
    return verifyToken(cookie);
}
