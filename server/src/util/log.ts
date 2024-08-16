function now() {
    const d = new Date();

    const year = d.getUTCFullYear();
    const month = String(d.getUTCMonth() + 1).padStart(2, '0');
    const day = String(d.getUTCDate()).padStart(2, '0');

    const hours = String(d.getUTCHours()).padStart(2, '0');
    const minutes = String(d.getUTCMinutes()).padStart(2, '0');
    const seconds = String(d.getUTCSeconds()).padStart(2, '0');
    const milliseconds = String(d.getUTCMilliseconds()).padStart(3, '0');

    return `${year}-${month}-${day} ${hours}:${minutes}:${seconds}.${milliseconds}`;
}

function log(msg: string) {
    console.log(`${now()}  ${msg.replace(/\r/g, '\\r').replace(/\n/g, '\\n').replace(/\t/g, '\\t')}`);
}

export function logWith(ctx: string, msg: string) {
    log(`[${ctx}] ${msg}`);
}
