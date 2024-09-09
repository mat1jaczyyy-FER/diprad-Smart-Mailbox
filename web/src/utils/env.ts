import type { Env } from "./types";

import secrets from '../../.secrets.json' assert { type: 'json' };

export default {
    maxAge: 604800,
    cookieName: "_Security_Login_",
    jwtSecret: secrets.jwtSecret,
    authPepper: secrets.authPepper,
    sqlconfig: secrets.sqlconfig,
    vapid: secrets.vapid
} as Env;
