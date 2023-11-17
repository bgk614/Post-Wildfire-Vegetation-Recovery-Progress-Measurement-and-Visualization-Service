document.getElementById('uploadForm').addEventListener('submit', async function(e) {
    e.preventDefault();
    const formData = new FormData(this);
    const response = await fetch('/upload', {
        method: 'POST',
        body: formData
    });

    const result = await response.json();
    if (result.success) {
        // JSON 문자열을 객체로 변환
        const data = JSON.parse(result.result);

        // 결과를 웹페이지에 표시
        displayResults(data);
    } else {
        console.error('Error in processing images');
    }
});

function displayResults(data) {
    document.getElementById('allPixel').innerText = 'All Pixels: ' + data.all_pixel;
    document.getElementById('greenPixel').innerText = 'Green Pixels: ' + data.green_pixel;
    document.getElementById('greenPercent').innerText = 'Green Percent: ' + data.green_percent + '%';
    document.getElementById('greenProgress').innerText = 'Green Progress: ' + data.green_progress + '%';

    // 새로 추가된 부분: 저장된 이미지 경로를 이용하여 이미지 표시
    const imagePaths = data.savedImages;
    for (let i = 0; i < imagePaths.length; i++) {
        document.getElementById(`imagePreview${i+1}`).querySelector('img').src = imagePaths[i];
    }
}


function previewImages() {
    // 이미지 미리보기를 위한 함수
    var images = document.querySelectorAll('input[type=file]');

    images.forEach(image => {
        if (image.files && image.files[0]) {
            var reader = new FileReader();
            reader.onload = function (e) {
                // 이미지에 해당하는 미리보기 엘리먼트의 src 속성을 설정
                var previewId = image.id + 'Display';
                document.getElementById(previewId).src = e.target.result;
                document.getElementById(previewId).style.display = 'block';
            };

            // 파일 읽기
            reader.readAsDataURL(image.files[0]);
        }
    });
}

// 각 파일 입력 필드에 미리보기 기능 연결
document.getElementById('pastImage').addEventListener('change', previewImages);
document.getElementById('fireImage').addEventListener('change', previewImages);
document.getElementById('1YearlaterImage').addEventListener('change', previewImages);
document.getElementById('presentImage').addEventListener('change', previewImages);

// script.js
document.getElementById('uploadForm').addEventListener('submit', async function(e) {
    e.preventDefault();
    const formData = new FormData(this);
    const response = await fetch('/upload', {
        method: 'POST',
        body: formData
    });

    const result = await response.json();
    if (result.success) {
        // 결과를 웹페이지에 표시
        displayResults(result.result);
    } else {
        console.error('Error in processing images');
    }
});
