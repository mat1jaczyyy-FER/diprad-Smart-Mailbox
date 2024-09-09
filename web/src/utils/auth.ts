import jwt from "jsonwebtoken";
import { db } from "./db";
import type { Token } from "./types";

// todo hide better in env?
const secret = "secret";

function signToken(username: string, id: number, maxAge: number) {
    return jwt.sign(
        {
            expires: Date.now() + maxAge * 1000,
            id: id,
            username: username
        } as Token,
        secret
    );
}

async function verifyUserID(username: string, password: string, isRegister: boolean): Promise<number | null> {
    const result = await db.request()
        .input('username', username)
        .input('password', password)
        .execute(isRegister? 'sp_user_register' : 'sp_user_login');
    
    const success = result.recordset[0]?.result?? 0;
    return success? success : null;
}

export async function tryCreateToken(username: string, password: string, maxAge: number) {
    const id = await verifyUserID(username, password, true);
    return id? signToken(username, id, maxAge) : null;
}

export async function tryGetToken(username: string, password: string, maxAge: number) {
    const id = await verifyUserID(username, password, false);
    return id? signToken(username, id, maxAge) : null;
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
