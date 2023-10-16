document.addEventListener("DOMContentLoaded", function () {
    const shareButton = document.getElementById("shareButton");
    const searchButton = document.getElementById("searchButton");
    const shareForm = document.getElementById("shareForm");
    const searchForm = document.getElementById("searchForm");
    const uploadProgress = document.getElementById("uploadProgress");
    const searchResults = document.getElementById("searchResults");

    const peer = new Peer({ key: 'your-peerjs-api-key' });

    peer.on('open', function (id) {
        console.log('My peer ID is: ' + id);
    });

    peer.on('connection', function (conn) {
        console.log('Connected to another peer');
        conn.on('data', function (data) {
            if (data.type === 'file') {
                const file = data.file; 
            } else if (data.type === 'search') {
                const query = data.query; 
            }
        });
    });

    const shareSubmit = document.getElementById('shareSubmit');
    const fileInput = document.getElementById('fileInput');

    shareSubmit.addEventListener('click', function () {
        const file = fileInput.files[0];
        if (file) {
            const connectedPeer = peer.connect('connectedPeerID');
            connectedPeer.on('open', function () {
                connectedPeer.send({ type: 'file', file: file });
            });
        }
    });

    const searchSubmit = document.getElementById('searchSubmit');
    const searchInput = document.getElementById('searchInput');

    searchSubmit.addEventListener('click', function () {
        const query = searchInput.value;
        if (query) {
            const connectedPeer = peer.connect('connectedPeerID');
            connectedPeer.on('open', function () {
                connectedPeer.send({ type: 'search', query: query });
            });
        }
    });

    shareButton.addEventListener("click", function () {
        shareForm.style.display = "block";
        searchForm.style.display = "none";
    });

    searchButton.addEventListener("click", function () {
        shareForm.style.display = "none";
        searchForm.style.display = "block";
    });
});
