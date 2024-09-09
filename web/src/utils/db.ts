import sql from 'mssql/msnodesqlv8.js';
import env from './env';

const db = new sql.ConnectionPool(env.sqlconfig);
await db.connect();

export { db };
