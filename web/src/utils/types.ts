export interface Token {
    expires: number;
    username: string;
    id: number;
};
export interface Env {
    maxAge: number;
    cookieName: string;
    secret: string;
}
