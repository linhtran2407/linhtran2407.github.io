var app = angular.module('app', []);

app.controller('MainController', ['$scope', function($scope) {
    $scope.experiences =
    [
        {
            title: 'Path-finding visualizer',
            text: "A visualizer tool for path-finding algorithms, created with JavaScript, Express.js, HTML/CSS. The tool allows user to create blocking wall between starting node and target node and choose an algorithm to visualize. The tool will animate the path-finding process. After the algorithm is done, the shortest path found will be highlighted. The whole project's credit goes to Clément Mihailescu - a great teacher whom I learn a tons from!",
            image: 'assets/img/vv.png',
            link: 'https://linhtran2407.github.io/PathFindingVisualizer/'
        },
        {
            title: 'Fake Caller',
            text: 'An Android app that is created with Java, Android Studio, and Back4App for data storage. The app allows Android users to make fake call during dangerous or uncomfortable situation to avoid being followed. Users can also schedule fake incoming call, or make real call to 911 in case of real emergency when the app alone cannot help. The idea came from my personal experience when I was usually cat-called and followed by uneducated men at night. I really hope that the app, when published, can provide some help to other girls/women like me.',
            image: 'assets/img/fakecaller.png',
            link: 'https://github.com/app-project1/fake-caller-app'
        },
        {
            title: 'Together',
            text: 'A web server developed using JavaScript, Node.js, Express.js, Mongoose, HTML/CSS. It allows users to ',
            image: 'assets/img/this.png',
            link: '#'
        },
        {
            title: 'Upsilon Pi Epsilon',
            text: 'I contributed to the web development committee at UPE, the Computer Science Honor Society at UC Berkeley. Me and a few others worked on adding new features to their website and improving the code base. Recently, we improved office hours such that officers could manage hours with a simple interface—no code knowledge needed. Also, we added the ability to schedule appointments for mock interviews and other things. It has been a great learning experience; I picked up some knowledge of the Django framework (and ultimately backend) here!',
            image: 'assets/img/upe.png',
            link: 'http://upe.berkeley.edu'
        },
        {
            title: 'Oppia',
            text: 'Founded by a few employees at Google, Oppia is an open-source project that aims to allow anyone to create interactive "explorations" and simulate one-on-one tutoring. I contributed to the project by adding infinite scrolling to the gallery on the website, picking up AngularJS in the process! Being only in the first semester at UC Berkeley at the time, I thought that this project was great and because of it, I was able to start contributing using my programming skills early on!',
            image: 'assets/img/oppia.png',
            link: 'https://www.oppia.org'
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
