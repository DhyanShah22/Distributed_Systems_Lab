const fs = require("fs");
const {setTimeout} = require("timers/promises")

let running = true;
let inCriticalSection = false
let processStatus = {}
let requestQueue = []

const log = (message) => {
    fs.appendFileSync('Ricart_Agarwala_output1.txt', message + '\n');
};

class Process {
    constructor(id){
        this.id = id;
        this.requesting = false;
        this.timestamp = 0;
        processStatus[id] = true;
    }

    async requestCriticalSection(ts) {
        this.requesting = true;
        this.timestamp = ts;
        log(`Process ${this.id} requested critical section at timestamp ${ts}`);
        requestQueue.push(this.id);
        await this.waitForCriticalSection();
    }

    releaseCriticalSection() {
        this.requesting = false;
        log(`Process ${this.id} released critical section`);
        inCriticalSection = false;
        requestQueue.shift(); 
    }

    async waitForCriticalSection() {
        while (inCriticalSection || requestQueue[0] !== this.id) {
            await new Promise((resolve) => setTimeout(resolve, 100)); 
        }
        inCriticalSection = true;
        log(`Process ${this.id} entered critical section`);
    }



    simulateFailure() {
        processStatus[this.id] = false;
        log(`Process ${this.id} failed`);
    }

    recover() {
        processStatus[this.id] = true;
        log(`Process ${this.id} recovered`);
    }
}

async function dynamicjoin(processes) {
    let newId = processes.length + 1;
    while(running){
        await setTimeout(100)
        const newProcess = new Process(newId);
        processes.push(newProcess)
        log(`Process ${newId} has joined`)
        processLifecycle(newProcess)
        newId++
    }
}

async function monitorProcess(processes){
    while(running){
        await setTimeout(100)
        processes.forEach((process) => {
            if(!processStatus[process.id]){
                process.recover()
            }
        });
    }
}

async function processLifecycle(process){
    for(let i =0; i < 3; i++){
        if(!processStatus[process.id]){
            await setTimeout(100)
            constinue;
        }
        const ts = performance.now()
        await process.requestCriticalSection(ts)
        await setTimeout(100)
        await process.releaseCriticalSection()
        await setTimeout(100)
    }
}

async function main() {
    const numProcesses = 3;

    const processes = [];

    for(let i = 1; i <= numProcesses; i++){
        const process = new Process(i)
        processes.push(process)
        processLifecycle(process)
    }

    dynamicjoin(processes)
    monitorProcess(processes)

    await setTimeout(10000)
    running = false
}

main().then(() => log('Simulation Ended!'))