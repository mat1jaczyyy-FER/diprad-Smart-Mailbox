export interface Token {
    expires: number;
    username: string | null;
};
export interface Env {
    maxAge: number;
    cookieName: string;
}
