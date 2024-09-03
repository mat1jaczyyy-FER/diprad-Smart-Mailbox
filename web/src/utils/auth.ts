import jwt from "jsonwebtoken";
import { db } from "./db";
import { internalAPI } from "./server";
import type { Token } from "./types";

// todo hide better in env?
const secret = "secret";

export async function tryCreateToken(username: string, password: string, maxAge: number) {
    const result = await db.request()
        .input('username', username)
        .input('password', password)
        .execute('sp_user_login');
    
    const success = result.recordset[0]?.result?? 0;
    if (!success) {
        return null;
    }

    return jwt.sign(
        {
            expires: Date.now() + maxAge * 1000,
            id: success,
            username: username
        } as Token,
        secret
    );
}

export function verifyToken(token?: string) {
    switch (token) {
        case "undefined":
        case undefined:
        case "null":
        case null:
            return null;
    }

    const data = jwt.verify(token, secret) as Token;
    if (!data.username)
        return null;

    const untilExpiry = (data.expires - Date.now()) / 1000;
    if (untilExpiry < 0)
        return null;

    return data;
}