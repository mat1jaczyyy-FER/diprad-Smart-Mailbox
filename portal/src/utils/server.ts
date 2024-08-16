import type { APIRoute } from 'astro';

const SERVER_URL = 'http://localhost:6245';

async function send(path: string, stringified: string) {
    return await fetch(`${SERVER_URL}/${path}`, {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
        },
        body: stringified
    });
}

export async function serverAPI(path: string, data: any) {
    return (await send(path, JSON.stringify(data))).json();
}

export function createMirror(path: string): APIRoute {
    return async ({request}) => {
        return await send(path, await request.text());
    }
}
