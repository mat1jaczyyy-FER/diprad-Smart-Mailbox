(async () => {

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

function log(msg) {
    console.log(`${now()}  ${msg.replace(/\r/g, '\\r').replace(/\n/g, '\\n').replace(/\t/g, '\\t')}`);
}

function logWith(ctx, msg) {
    log(`[${ctx}] ${msg}`);
}

const sqlconfig = require('./.sqlconfig.json');
const sql = require('mssql/msnodesqlv8');

const sqlPool = new sql.ConnectionPool(sqlconfig);
await sqlPool.connect();

const express = require('express');
const app = express();

app.use(require('body-parser').json());
app.use((req, res, next) => {
    res.setHeader('Content-Type', 'application/json');
    next();
});

app.get('/api/test', async (req, res) => {
    res.json({ prop: 'value', now: now() });
});

app.listen(process.env.PORT || 6245, () => log('Server started'));

})();
