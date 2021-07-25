var app = angular.module('app', []);

app.controller('MainController', ['$scope', function($scope) {
    $scope.projects =
    [
        {
            title: 'Path-finding visualizer',
            text: "A visualizer tool for path-finding algorithms, created with JavaScript, React, HTML/CSS. The tool allows user to create blocking wall between starting node and target node and choose an algorithm to visualize. The tool will animate the path-finding process. After the algorithm is done, the shortest path found will be highlighted. The whole project's credit goes to Clément Mihailescu - a great teacher whom I learn a tons from!",
            image: 'assets/img/pathfindingvisualizer.png',
            link: 'https://linhtran2407.github.io/PathFindingVisualizer/'
        },
        {
            title: 'Fake Caller',
            text: 'An Android app that is created with Java, Android Studio, and Back4App for data storage. The app allows Android users to make fake call during dangerous or uncomfortable situation to avoid being followed. Users can also schedule fake incoming call, or make real call to 911 in case of real emergency when the app alone cannot help. The idea came from my personal experience when I was usually cat-called and followed by uneducated men at night. I and my team really hope that the app, when published, can provide some help to other girls/women like me.',
            image: 'assets/img/fakecaller.png',
            link: 'https://github.com/app-project1/fake-caller-app'
        },
        {
            title: 'Together',
            text: 'A web server developed using JavaScript, Node.js, Express.js, Mongoose, HTML/CSS. It allows users to create account, pair with other to make a couple, share and write diaries together, count together-days, and send real-time messages. This was my first project with Node.js and Express.js, which helped me learn really effectively the process of building a web server.',
            image: 'assets/img/together.png',
            link: '#'
        },
        {
            title: 'Personal website',
            text: 'Yes, I do refer to this website as a project. This was built by JavaScript, Angular.js, and jQuery, which together provide me this single-page, simple, yet informative-enough website. It was a fun experience building this project.',
            image: 'assets/img/this.png',
            link: '#'
        },
        {
            title: 'Mouse in a maze',
            text: ' The UI of the game was developed initially by my professor David Wonnacott at Haverford College. The idea of the game was simple since it is supposed to help players kill free time. I then added algorithm, which was written in Python, such that it guarantees that the mouse will always successfully reach the cheese without getting stuck in the maze regardless of the size of the maze, which can be changed by typing in a custom size that the users want (the larger the size, the more complicated the maze is). When the users click on "Do 1 Step", the mouse will only move 1 step forward the cheese at the end. When the users click "Find the Cheese!", the mouse is immediately brought to the cheese.',
            image: 'assets/img/mouseinamaze.png',
            link: 'https://github.com/linhtran2407/MouseInAMaze'
        },
    ];
    $scope.interests =
    [
        {
            name: 'The Verge',
            text: 'One of the top sites to go to for technology news. Been following it since it was "This is my Next".',
            link: 'http://www.theverge.com/',
            image: 'assets/img/theverge.png'
        },
        {
            name: 'MacRumors',
            text: 'Good source of news about Apple-related things. Forums are useful, too. Discovered it during a history day project years ago.',
            link: 'http://www.macrumors.com/',
            image: 'assets/img/macrumors.png'
        },
        {
            name: 'iDownloadBlog',
            text: 'Very useful for finding out all that\'s needed for jailbreaks and the like.',
            link: 'http://www.idownloadblog.com/',
            image: 'assets/img/idownloadblog.png'
        },
        {
            name: 'IGN',
            text: 'The reviews that IGN gives on my favorite shows (like Arrow and Flash) are places for interesting discussions.',
            link: 'http://www.ign.com/',
            image: 'assets/img/ign.png'
        }
    ];
}]);
