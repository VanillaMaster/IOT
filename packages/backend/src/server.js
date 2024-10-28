/**@import { ServerResponse } from "node:http" */
import { createServer } from "node:http"
import { arrayBuffer, text } from "node:stream/consumers"
import Router from "find-my-way"
import { pipeline } from "node:stream";
import { createReadStream } from "node:fs"
import {  } from "node:path"

const router = Router({
    defaultRoute(req, res) {
        res.writeHead(404);
        res.end();
    }
});

const server = createServer(function (req, res) {
    router.lookup(req, res);
});

router.post("/data", async function (req, res) {
    const hex = await text(req);
    const bytes = new Uint8Array(Buffer.from(hex, "hex"));
    const [
        h,
        t,
        ft,
        hift,
        hict
    ] = new Float32Array(bytes.buffer);
    for (const client of clients) {
        client.write(`data: ${JSON.stringify({ h, t, ft, hift, hict })}\n\n`);
    }
    // console.log(h, t, ft, hift, hict);
    res.end();
});

/**@type { Set<ServerResponse> } */
const clients = new Set();

router.get("/data", async function (req, res) {
    res.writeHead(200, {
        'Content-Type': 'text/event-stream',
        'Connection': 'keep-alive',
        'Cache-Control': 'no-cache'
    });
    clients.add(res);
    res.on("close", function () {
        clients.delete(res);
    })
    res.on("error", function () {
        clients.delete(res);
    });
});

router.get("/", function(req, res) {
    res.setHeader("Content-Type", "text/html")
    pipeline(
        createReadStream(new URL("../res/index.html", import.meta.url)),
        res,
        (err) => { if (err) console.error(err); }
    );
})

server.listen(8080);