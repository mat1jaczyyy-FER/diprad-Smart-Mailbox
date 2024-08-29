import { defineConfig } from 'astro/config';
import node from "@astrojs/node";
import sitemap from "@astrojs/sitemap";

const ignore = [];

// https://astro.build/config
export default defineConfig({
    output: "server",
    adapter: node({
        mode: "standalone"
    }),
    site: 'https://smart-mailbox.mat1jaczyyy.com',
    redirects: {},
    integrations: [
        sitemap({
            filter: page => ignore
                .map(i => `https://smart-mailbox.mat1jaczyyy.com/${i}`)
                .every(i => !page.startsWith(i)),
        })
    ]
});
