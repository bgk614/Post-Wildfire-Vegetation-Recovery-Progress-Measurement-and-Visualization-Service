document.getElementById('Image').addEventListener('click', function() {
    fetch('/delete-images', { method: 'POST' })
        .then(response => response.json())
        .then(data => {
            console.log('Images deleted:', data.message);
        })
        .catch(error => console.error('Error:', error));
});

document.getElementById('uploadForm').addEventListener('submit', async function(e) {
    e.preventDefault();
    const formData = new FormData(this);
    const response = await fetch('/uploads', { method: 'POST', body: formData });

    const result = await response.json();
    if (result.success) {
        const data = JSON.parse(result.result); 
        displayResults(data);                   
        displayImages(data.green_pixel.length);
        chart(data);
    } else {
        console.error('Error in processing images');
    }
});

function displayResults(data) {
    document.getElementById('allPixel').innerText = 'All Pixels: ' + data.all_pixel
    document.getElementById('greenPixel').innerText = 'Green Pixels: ' + data.green_pixel.join(' / ');
    document.getElementById('greenPercent').innerText = 'Green Similar Percent: ' + data.green_similar_percent.join(' / ');
    document.getElementById('greenProgress').innerText = 'Green Progress: ' + data.green_progress.join(' / ');
}

function displayImages(greenPixelCount) {
    const srcImageContainer = document.querySelector('.srcImageContainer');
    const greenImageContainer = document.querySelector('.greenImageContainer');
    const changeImageContainer = document.querySelector('.changeImageContainer');
    
    srcImageContainer.innerHTML = '';
    greenImageContainer.innerHTML = '';
    changeImageContainer.innerHTML = '';

    for (let i = 1; i < greenPixelCount + 1; i++) {
        const srcImg = new Image();
        srcImg.src = `./srcImage/srcImage_${i}.jpg`;
        srcImageContainer.appendChild(srcImg);

        const greenImg = new Image();
        greenImg.src = `./greenPixelImage/greenArea_${i}.jpg`;
        greenImageContainer.appendChild(greenImg);

        if (i < greenPixelCount) {
            const changeImg = new Image();
            changeImg.src = `./changeImage/changeArea_${i}_${i+1}.jpg`;
            if (i === 1) changeImg.style.marginLeft = '148px'; 
            changeImageContainer.appendChild(changeImg);
        
        }
    }
}

var greenPixelChartInstance;
var similarPercentChartInstance;
var greenProgressChartInstance;

function chart(data) {
    if (greenPixelChartInstance) {
        greenPixelChartInstance.destroy();
    }
    if (similarPercentChartInstance) {
        similarPercentChartInstance.destroy();
    }
    if (greenProgressChartInstance) {
        greenProgressChartInstance.destroy();
    }

    const greenPixelChart = document.getElementById('greenPixelChart').getContext('2d');
    greenPixelChartInstance = new Chart(greenPixelChart, {
        type: 'line', 
        data: {
            labels: data.green_pixel.map((_, i) => `Image ${i+1}`), 
            datasets: [{
                    label: 'Green Pixels',
                    data: data.green_pixel, 
                    backgroundColor: 'rgba(45, 192, 72)', 
                    borderColor: 'rgba(45, 192, 72)', 
                    borderWidth: 0.5,
                    lineTension: 0.2
                }, {
                    type: 'bar',
                    label: 'Green Pixels',
                    data: data.green_pixel, 
                    backgroundColor: 'rgba(30, 50, 30, 0.1)', 
                    borderWidth: 0.5,
                }]
        },
        options: {scales: {y: {beginAtZero: true}}}
    })
    const similarPercentChart = document.getElementById('similarPercentChart').getContext('2d');
    similarPercentChartInstance = new Chart(similarPercentChart, {
        type: 'line', 
        data: {
            labels: data.green_similar_percent.map((_, i) => `Image ${i+2}`), 
            datasets: [{
                    label: 'Green Similar Percent',
                    data: data.green_similar_percent, 
                    backgroundColor: 'rgba(107, 102, 155)', 
                    borderColor: 'rgba(107, 102, 155)', 
                    borderWidth: 0.5,
                    lineTension: 0.2
                }, {
                    type: 'bar',
                    label: 'Green Similar Percent',
                    data: data.green_similar_percent, 
                    backgroundColor: 'rgba(30, 50, 30, 0.1)', 
                    borderWidth: 0.5,
                }]
        },
        options: {scales: {y: {beginAtZero: true }}}
        })
    const greenProgressChart = document.getElementById('greenProgressChart').getContext('2d');
    greenProgressChartInstance = new Chart(greenProgressChart, {
        type: 'line', 
        data: {
        labels: data.green_progress.map((_, i) => `Image ${i+2}`), 
        datasets: [{
                label: 'Green Progress',
                data: data.green_progress, 
                backgroundColor: 'rgba(255, 114, 0)', 
                borderColor: 'rgba(255, 114, 0)', 
                borderWidth: 0.5,
                lineTension: 0.2
                }, {   
                type: 'bar', 
                label: 'Green Progress',
                data: data.green_progress, 
                backgroundColor: 'rgba(30, 50, 30, 0.1)', 
                borderWidth: 0.5,
                }]
            },
            options: {
                scales: {
                    y: {beginAtZero: true }
                }
            }
        })
}
