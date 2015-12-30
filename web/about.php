<!DOCTYPE html>
<html>
  <meta charset="utf-8">
  <meta http-equiv="X-UA-Compatible" content="IE=edge">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <meta name="author" content="Steve Göring">
  <?php
    include_once('header.php');
  ?>
  <script type="text/javascript">
  <?php
    include_once("config.json");
  ?>

  function add_server_address() {
      // create list of servers
      for(var i = 0; i < config["server_url"].length; i++) {
          var server = "<li><a href=\"" + config["server_url"][i] + "\" > " + config["server_url"][i] + "</a></li>";
          console.log(server);
          $(".server-instances").append(server);
      }

      // update link to first server with defined query
      $(".api").each(function() {
          el = $(this);
          el.prop("href", config["server_url"][0] + el.text());
      });

  }

  $().ready(function() {
      build_toc();
      add_server_address();
  });

  </script>
  <body>
    <?php
      include_once('nav.php');
    ?>

    <div class="maincontent">
    <section class="container theme-showcase" role="main" style="height:80%">

        <div class="results">
        <article>
        <h2 id="about">About</h2>
        <p>
        PhrasIt is an open source clone of <a href="http://www.netspeak.org/">Netspeak</a>
        developed for experiments (other datasets, other operators, larger ngrams, ...), where a local installed 'writing lookup' service is needed.
        All this possible extensions cannot be done with Netspeak, because it is just a webservice for the google-web-ngram dataset and
        no source code is available.
        In the last years there were no visible development changes or improvements for Netspeak.
        The idea for PhrasIt is to create a similar system, so that changes and new ideas can easily be implemented.
        E.g. you can build your own service using a german ngram dataset, or build a PhrasIt instance using scientific papers as source.
        </p>
        <div class="alert alert-success" role="alert">Languages are diverse, so even tools for languages should it be!</div>
        <p>
        This web-service is not optimized for speed, it is just a demonstration of a live system.
        </p>
        <p>
        As a data-source PhrasIt uses the <a href="https://books.google.com/ngrams">google-book-ngram</a> dataset, because this dataset is free available,
        in contrast to the google-web-ngram dataset that Netspeak uses.
        But in general if you build your own PhrasIt you can use every suitable data-source, e.g. you can build your own dataset,
        for more information about other datasets you should have a look at <a href="#develop" >development</a>.
        </p>
        <p>
        PhrasIt can be used as a tool to assist writers that are no native speakers (not only for English).
        E.g. if you write an English article or scientific paper and you are not sure with the ordering of a few words, you can use
        PhrasIt for order checking, or if you are not sure which word fits as best in the context.
        </p>
        <h3 id="query-operators">Query Operators</h3>
        <p>
        It is possible to submit different kinds of queries to PhrasIt. All operators will be summarized in the following table.
        <div class="table-responsive">
            <table class="table">
              <thead>
                <tr>
                    <th>Query operator</th>
                    <th>Description</th>
                    <th>Example</th>
                </tr>
              </thead>
              <tbody>
                <tr>
                  <td>QMARK, ?</td>
                  <td>Look for n-grams that match query where ? can be every word.</td>
                  <td><pre>hello ?</pre> <pre>? world</pre> for both you should get 'hello world' as result </td>
                </tr>
                <tr>
                  <td>ASTERISK, *</td>
                  <td>Look for n-grams that match query where * can be many words (e.g. from no word up to 4 words).</td>
                  <td><pre>hello *</pre> <pre>* world</pre> both will hopefully return 'hello world', 'hello my world' </td>
                </tr>
                <tr>
                  <td>OPTIONSET, []</td>
                  <td>Get n-grams that match query where all words in [] where checked.</td>
                  <td><pre>hello [world, what]</pre> will return 'hello world' as best matching candidate </td>
                </tr>
                <tr>
                  <td>ORDERSET, {}</td>
                  <td>Check ordering of words that are in {}.</td>
                  <td><pre>{world, hello}</pre> should generate 'hello world' as best matching candidate </td>
                </tr>
              </tbody>
            </table>
        </div>

        </p>
        <h3 id="complex-example">Complex Examples</h3>
        <p>
        All operators can be combined and used in complex constructed queries.
        <div class="table-responsive">
            <table class="table">
              <thead>
                <tr>
                    <th>Query</th>
                    <th>Results</th>
                    <th>Description</th>
                </tr>
              </thead>
              <tbody>
                <tr>
                  <td><pre>world outside [of, in] science fiction</pre></td>
                  <td><a href="./results.php?query=<?php echo urlencode("world outside [of, in] science fiction");?>">query</a></td>
                  <td>you are not sure which word is correct</td>
                </tr>
                <tr>
                  <td><pre>humans ? intelligent</pre></td>
                  <td><a href="./results.php?query=<?php echo urlencode("humans ? intelligent");?>">query</a></td>
                  <td>you are not sure which word fits best</td>
                </tr>
                <tr>
                  <td><pre>{a, time, ago, long} in</pre></td>
                  <td><a href="./results.php?query=<?php echo urlencode("{a, time, ago, long} in");?>">query</a></td>
                  <td>you don't know the ordering</td>
                </tr>
                <tr>
                  <td><pre>a long *</pre></td>
                  <td><a href="./results.php?query=<?php echo urlencode("a long *");?>">query</a></td>
                  <td>what will be the next most used words in this context</td>
                </tr>
                <tr>
                  <td><pre>a * [in,of] ? </pre></td>
                  <td><a href="./results.php?query=<?php echo urlencode("a * [in,of] ? ");?>">query</a></td>
                  <td>a combined query</td>
                </tr>
                <tr>
                  <td><pre>{it,do} * not </pre></td>
                  <td><a href="./results.php?query=<?php echo urlencode("{it,do} * not");?>">query</a></td>
                  <td>another combined query</td>
                </tr>
              </tbody>
            </table>
        </div>
        </p>

        <h3 id="api">Using the API</h3>
        <p>
        If you want to use the PhrasIt web-service in another application you can perform
        API calls directly to a running PhrasIt server instance.
        </p>
        <p>
        Configured server instances are:
        <ul class="server-instances">
        </ul>

        </p>
        And now you can perform a few example queries on one of the configured server:
        <ul>
            <li><a href="" class="api">?query=hello+?</a></li>
            <li><a href="" class="api">?query=hello+*&callback=handler</a></li>

        </ul>
        General every api call will return a json formatted result, if you specify a callback
        function, then you will get a jsonp result with callback.
<pre>
<?php
  include_once("example.json");
?>
</pre>

        <h3 id="develop">Development</h3>
        <p>
        For development and setting up a local PhrasIt server please look at
        the <a href="http://github.com/stg7/phrasit">github project page</a>.
        </p>
        </article>
        </div>

    </section>
    </div>
    <?php
      include_once('footer.php');
    ?>

  </body>

</html>


