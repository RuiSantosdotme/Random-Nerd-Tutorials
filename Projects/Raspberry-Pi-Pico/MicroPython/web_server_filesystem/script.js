// Turn LED ON
async function turnOn() {
    try {
        const response = await fetch('/lighton');
        const data = await response.json();
        document.getElementById('led-status').textContent = data.state;
    } catch (error) {
        console.error('Error turning LED ON:', error);
        alert('Failed to turn LED ON');
    }
}

// Turn LED OFF
async function turnOff() {
    try {
        const response = await fetch('/lightoff');
        const data = await response.json();
        document.getElementById('led-status').textContent = data.state;
    } catch (error) {
        console.error('Error turning LED OFF:', error);
        alert('Failed to turn LED OFF');
    }
}

// Get new temperature reading
async function getNewReading() {
    try {
        const response = await fetch('/temperature');
        
        const data = await response.json();
        
        document.getElementById('temp_c').textContent = data.temperature_c;
        document.getElementById('temp_f').textContent = data.temperature_f;
    } catch (error) {
        console.error('Error fetching temperature:', error);
        alert('Failed to get new reading');
    }
}
