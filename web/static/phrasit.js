

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
