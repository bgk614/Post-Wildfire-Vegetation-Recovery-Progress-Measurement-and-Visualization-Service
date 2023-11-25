const express = require('express');
const multer = require('multer');
const { execFile } = require('child_process');
const fs = require('fs');
const path = require('path');
const app = express();
const port = 3000;
const uploads = multer({ dest: 'uploads/' });

app.use(express.static('./project'));

app.post('/uploads', uploads.array('images'), (req, res) => {
    const imagePaths = req.files.map(file => file.path);
    execFile('./project/test_program', imagePaths, (error, stdout, stderr) => {
        if (error) {
            console.error('stderr', stderr);
            res.status(500).send('Error in image processing');
            return;
        }
        const result = JSON.parse(stdout);
        res.json({ success: true, result: stdout });
    });
});

app.post('/delete-images', (req, res) => {
    deleteImagesInFolder('./project/srcImage');
    deleteImagesInFolder('./project/greenPixelImage');
    deleteImagesInFolder('./project/changeImage');
    res.json({ success: true, message: 'All images have been deleted' });
});

function deleteImagesInFolder(folderPath) {
    const files = fs.readdirSync(folderPath);
    for (const file of files) {
        fs.unlinkSync(path.join(folderPath, file));
    }
}

app.listen(port, () => {
console.log(`Server listening at http://localhost:${port}`);
});
