const fs = require("fs");

const numProcesses = 3;
const numIterations =5;
const vectorClocks = Array.from({length: numProcesses}, () => Array(numProcesses).fill(0))
const eventOrders = Array.from({length: numProcesses}, () => []);
const messageBuffers = Array.from({length: numProcesses}, () => [])
const delay = (ms) => new Promise((resolve) => setTimeout(resolve, ms))

function logEvent(processId, eventOrder){
    const eventString = `process ${processId} event order: ${JSON.stringify(eventOrder)}\n`
    console.log(eventString.trim());
    fs.appendFileSync("Output_final.txt", eventString)
}

async function processFunction(processId){
    for (let i = 0; i < numIterations; i++){
        await delay(Math.random() * 200)
        vectorClocks[processId][processId]++;
        eventOrders[processId].push([...vectorClocks[processId]])

        const recipientId = (processId + 1) % numProcesses;
        messageBuffers[recipientId].push([...vectorClocks[processId]])

        if(messageBuffers[processId].length > 0){
            const receivedClock = messageBuffers[processId].shift();
            for(let j = 0; j < numProcesses; j++){
                vectorClocks[processId][j] = Math.max(vectorClocks[processId][j], receivedClock[j])
            }
            vectorClocks[processId][processId]++;
            eventOrders[processId].push([...vectorClocks[processId]])
        }
        logEvent(processId, eventOrders[processId])
    }

}

async function main() {
    fs.writeFileSync("Output_final.txt", "")

    console.log(`Starting ${numProcesses} process with ${numIterations} iterations each.`)
    const processes = Array.from({length: numProcesses}, (_, i) => processFunction(i))
    await Promise.all(processes)

    console.log('\n Final Vector Clocks')
    vectorClocks.forEach((clock, i) => {
        console.log(`Process ${i}: [${clock.join(". ")}]`)
        fs.appendFileSync("Output_final.txt", `Process ${i}: [${clock.join(". ")}]`)
    })
}

main().catch(console.error)