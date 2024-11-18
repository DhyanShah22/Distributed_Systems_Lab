const fs = require("fs")
const { setTimeout } = require('timers/promises')

const logfile = "token_output.txt"
fs.writeFileSync(logfile, "")
let running = true

class Process{
    constructor(id, hasToken= false) {
        this.id = id
        this.hasToken = hasToken
        this.inCriticalSection = false
        this.nextProcess = null
    }

    async requestCriticalSection() {
        while(!this.hasToken){
            await setTimeout(100);
        }
        this.enterCriticalSection()
        await setTimeout(100);
        this.exitCriticalSection()
        this.passToken()
    }

    enterCriticalSection(){
        this.inCriticalSection = true;
        this.log('Entered the Critical Section!')
    }

    exitCriticalSection() {
        this.inCriticalSection = false;
        this.log('Exited the critical section')
    }

    passToken(){
        this.hasToken = false
        if(this.nextProcess){
            this.nextProcess.recieveToken()
        }
    }

    recieveToken(){
        this.hasToken = true;
    }

    log(message){
        const logEntry = `Process ${this.id} ${message}\n`
        console.log(logEntry.trim())
        fs.appendFileSync(logfile, logEntry)
    }
}

async function simulateProcess(process){
    for(let i =0; i < 3; i++){
        if(!running) return;

        await process.requestCriticalSection();
        await setTimeout(100);
    }
}
async function dynamicJoin(processes){
    let newId = processes.length + 1;

    while(running){
        await setTimeout(1000)
        const newProcess = new Process(newId)
        const lastProcess = processes[processes.length -1]
        lastProcess.nextProcess = newProcess
        newProcess.nextProcess = processes[0]
        processes.push(newProcess)

        newProcess.log("joined the system");
        simulateProcess(newProcess); 
        newId++;
    }
}

async function main() {
    const p1 = new Process(1, true)
    const p2 = new Process(2)
    const p3 = new Process(3)

    const processes = [p1, p2, p3]

    p1.nextProcess = p2
    p2.nextProcess = p3
    p3.nextProcess = p1


    processes.forEach(simulateProcess)

    dynamicJoin(processes)

    await setTimeout(10000);
    running = false; 
}

main().then(() => console.log("Simulation completed."));
