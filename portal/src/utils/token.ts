import jwt from "jsonwebtoken";
import type { Token } from "./types";

// todo hide better in env?
const secret = "secret";

export function createToken(username: string, password: string, maxAge: number) {
    // todo: check if username and password are valid in DB
    const success = username === "mat1jaczyyy" && password === "qweasdzxc";

    if (!success) {
        return null;
    }

    return jwt.sign(
        {
            expires: Date.now() + maxAge * 1000,
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

    // todo check expire

    return data;
}
