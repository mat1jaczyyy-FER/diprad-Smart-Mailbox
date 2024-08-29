import sql from 'mssql/msnodesqlv8.js';

import sqlconfig from '../../.sqlconfig.json' assert { type: 'json' };
const SQLConfig = sqlconfig as sql.config;

const db = new sql.ConnectionPool(SQLConfig);
await db.connect();

export { db };
