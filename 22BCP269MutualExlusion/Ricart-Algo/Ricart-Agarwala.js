const { performance } = require('perf_hooks');

let inCriticalSection = false;
let requestQueue = [];
let running = true;

const log = (message) => {
    console.log(message);
};

const setTimeoutAsync = (ms) => new Promise((resolve) => setTimeout(resolve, ms));

class Process {
    constructor(id) {
        this.id = id;
        this.requesting = false;
        this.timestamp = 0;
    }

    async requestCriticalSection() {
        this.requesting = true;
        this.timestamp = performance.now();
        log(`Process ${this.id} requested critical section at timestamp ${this.timestamp}`);
        requestQueue.push(this.id);
        await this.enterCriticalSection();
    }

    releaseCriticalSection() {
        this.requesting = false;
        log(`Process ${this.id} released critical section`);
        inCriticalSection = false;
        requestQueue.shift();
    }

    async enterCriticalSection() {
        while (inCriticalSection || requestQueue[0] !== this.id) {
            await setTimeoutAsync(100);
        }
        inCriticalSection = true;
        log(`Process ${this.id} entered critical section`);
    }
}

async function processLifecycle(process) {
    for (let i = 0; i < 3; i++) {
        await process.requestCriticalSection();
        await setTimeoutAsync(100);
        process.releaseCriticalSection();
        await setTimeoutAsync(100);
    }
}

async function main() {
    const numProcesses = 3;
    const processes = [];

    for (let i = 1; i <= numProcesses; i++) {
        const process = new Process(i);
        processes.push(process);
        processLifecycle(process);
    }

    await setTimeoutAsync(10000);
    running = false;
    log('Simulation ended.');
}

main();
