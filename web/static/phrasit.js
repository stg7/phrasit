function shuffle(array) {
    var currentIndex = array.length, temporaryValue, randomIndex;

    // While there remain elements to shuffle...
    while (0 !== currentIndex) {

        // Pick a remaining element...
        randomIndex = Math.floor(Math.random() * currentIndex);
        currentIndex -= 1;

        // And swap it with the current element.
        temporaryValue = array[currentIndex];
        array[currentIndex] = array[randomIndex];
        array[randomIndex] = temporaryValue;
    }

    return array;
}

function build_toc() {
    var ToC = "<nav role='navigation' class='table-of-contents'><ul>";

    $("article h3").each(function() {
        var newLine, el, title, link;
        el = $(this);
        title = el.text();
        link = "#" + el.attr("id");

        newLine = "<li><a href='" + link + "'>" + title + "</a></li>";
        ToC += newLine;

    });

    ToC += "</ul></nav><hr>";

    $("article").prepend(ToC);
}

function attachListener() {

    $(".example-query").click(function(e) {
        e.preventDefault();
        $("input[name=query]")[0].value = e.target.text;
    });

    $(".dataset-selector").click(function(e) {
        e.preventDefault();
        console.log("here" + e.target.text);
        console.log($(".dataset"))
        $(".dataset").text(e.target.text);
        $("input[name=dataset]")[0].value = e.target.text;
    });
}


$().ready(function() {
    attachListener();

    }
);
