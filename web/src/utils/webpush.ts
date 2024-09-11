import webpush from 'web-push';
import { db } from "./db";
import env from "./env";

webpush.setVapidDetails(
    'mailto:mat1jaczyyy@gmail.com',
    env.vapid.publicKey,
    env.vapid.privateKey
);

export default webpush;
