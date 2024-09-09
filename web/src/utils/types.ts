import type { config } from 'mssql/msnodesqlv8.js';

export interface Token {
    expires: number;
    username: string;
    id: number;
};

export interface Env {
    maxAge: number;
    cookieName: string;
    jwtSecret: string;
    authPepper: string;
    sqlconfig: config;
    vapid: {
        publicKey: string;
        privateKey: string;
    };
}
