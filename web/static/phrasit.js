
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
}


$().ready(function() {
    attachListener();

    }
);
