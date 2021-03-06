<!DOCTYPE html>
<html>
  <meta charset="utf-8">
  <meta http-equiv="X-UA-Compatible" content="IE=edge">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <meta name="author" content="Steve Göring">
  <?php
    include_once('header.php');
  ?>

  <script>
  (function($) {
      $.QueryString = (function(a) {
          if (a == "") return {};
          var b = {};
          for (var i = 0; i < a.length; ++i)
          {
              var p=a[i].split('=');
              if (p.length != 2) continue;
              b[p[0]] = decodeURIComponent(p[1].replace(/\+/g, " "));
          }
          return b;
      })(window.location.search.substr(1).split('&'))
  })(jQuery);

  var query = $.QueryString["query"];
  var dataset = $.QueryString["dataset"];
  console.log(query);

  // read config
  <?php
    include_once("config.json");
  ?>


  function get_server_url() {
      urls = shuffle(config["server_url"][dataset]);
      // TODO(stg7) perform a server reachable check!
      return urls[0];
  }

  // update content
  $(document).ready(function() {
    $(".query").text(query);
    $(".results").css('visibility','hidden');
    $("input[name=query]")[0].value = query;
    $("input[name=dataset]")[0].value = dataset;
    $(".dataset").text(dataset);

    var server_url = get_server_url();
    if (server_url == "") {
      console.log("no server reachable");
    } else {
      var url = server_url + "?query=" + query;
      var max_value = 0;
      $.ajax({
          type: 'GET',
          url: url,
          jsonpCallback: 'callback',
          async: true,
          contentType: "application/json",
          dataType: 'jsonp',
          success: function(json) {
              json["result"].forEach(function(r){
                $.each(r, function(key, value) {
                    if (max_value == 0) {
                        max_value = value;
                    }
                    // niceer result presentation
                    rel_value = (100 * value / max_value).toFixed();
                    if (rel_value < 10) {
                        rel_value = (100 * value / max_value).toFixed(1);
                    }
                    if (rel_value < 1) {
                        rel_value = (100 * value / max_value).toFixed(2);
                    }
                    var new_row = "<tr> <td>" + key +  " </td>  <td> " + rel_value + "%  </td> <td>" + value + "</td> </tr>";
                    $("tbody[id=results]").append(new_row);
                });

              });
              $(".time").text("needed time: " + json["time"]);
              $(".server").text("server: " + server_url);
              $(".results").css('visibility','visible');
          }
      });
    }
  });

  </script>

  <body>
    <?php
      include_once('nav.php');
    ?>

    <div class="maincontent">
      <section class="container theme-showcase" role="main" style="height:80%">

      <?php
          include_once('searchform.php');
      ?>
      <div class="results">
        <hr />

        Search results for <em class="query">query</em>

        <?php
          include_once('result.php');
        ?>
        <div>
          <text class="time"></text>ms <br><text class="server"></text>
        </div>
      </div>

      </section>
    </div>
    <?php
      include_once('footer.php');
    ?>

  </body>

</html>



