let emergencyQueue = [];
let normalQueue = [];
let records = {};

function addPatient() {
    let id = pid.value;
    let name = pname.value;
    let severity = parseInt(document.getElementById("severity").value);
    let type = ptype.value;

    if (!id || !name) {
        alert("Please enter all details");
        return;
    }

    let patient = {
        id, name, severity,
        arrival: new Date().toLocaleTimeString()
    };

    records[id] = patient;

    if (type === "emergency") {
        emergencyQueue.push(patient);
        emergencyQueue.sort((a, b) => b.severity - a.severity);
    } else {
        normalQueue.push(patient);
    }

    output.innerHTML = `Patient ${name} added successfully.`;
}

function treatPatient() {
    let patient;

    if (emergencyQueue.length > 0) {
        patient = emergencyQueue.shift();
    } else if (normalQueue.length > 0) {
        patient = normalQueue.shift();
    } else {
        output.innerHTML = "No patients to treat.";
        return;
    }

    output.innerHTML = `
        <strong>Patient Treated:</strong><br>
        ID: ${patient.id}<br>
        Name: ${patient.name}<br>
        Arrival: ${patient.arrival}
    `;
}

function viewQueues() {
    output.innerHTML = `
        Emergency Queue: ${emergencyQueue.length}<br>
        Normal Queue: ${normalQueue.length}
    `;
}

function searchPatient() {
    let id = searchId.value;
    let patient = records[id];

    if (!patient) {
        output.innerHTML = "Patient not found.";
        return;
    }

    output.innerHTML = `
        <strong>Patient Record:</strong><br>
        ID: ${patient.id}<br>
        Name: ${patient.name}<br>
        Severity: ${patient.severity}<br>
        Arrival: ${patient.arrival}
    `;
}
