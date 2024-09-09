import type { Env } from "./types";

export default {
    maxAge: Number(import.meta.env.MAX_AGE || "604800"),
    cookieName: import.meta.env.COOKIE_NAME || "_Security_Login_",
    secret: import.meta.env.SECRET || "spicypeppersecret"
} as Env;
