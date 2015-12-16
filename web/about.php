<!DOCTYPE html>
<html>
  <meta charset="utf-8">
  <meta http-equiv="X-UA-Compatible" content="IE=edge">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <meta name="author" content="Steve Göring">
  <?php
    include_once('header.php');
  ?>

  <body>
    <?php
      include_once('nav.php');
    ?>

    <div class="maincontent">
    <section class="container theme-showcase" role="main" style="height:80%">

        <div class="results">
        <article>
        <h2>About</h2>
        PhrasIt (<font class="em">re</font>ranking <font class="em">l</font>earned with <font class="em">ax</font>i<font class="em">o</font>ms)  is a learning to rank approach using axioms on top of Chatnoir2 created by <a href="https://github.com/stg7" >Steve Göring</a> 2015 at <a href="http://www.webis.de">webis group</a>.
        <h3>General Appraoch</h3>
        <p>
        After sending a query to relaxo, an initial document set will be retrieved by chatnoir2.
        Based on this document set the reranking approach using axioms is performed.
        Only the top-10 results will be reranked, because of performance reasons.
        </p>

        <h3>Api</h3>
        <ul>
            <li>./api/rerank?query=QUERY&topk=3 will perform a reranking of a QUERY with top-3 documents, e.g. <a href="api/rerank?query=star+trek&topk=3">example star trek</a></li>
            <li>./api/rerank?query=QUERY&dataset=clueweb09 will perform a reranking of a QUERY using clueweb09 corpora, default dataset is clueweb12, e.g. <a href="./api/rerank?query=star+trek&dataset=clueweb09">example star trek clueweb09</a></li>

            <li>./raw?id=DOCID will return the raw document of DOCID, where DOCID is a valid Clueweb09/12 document id, e.g. <a href="./raw?id=clueweb09-en0003-28-02624">example star trek</a></li>

        </ul>
        General every api call will return a json formatted result.
<pre>
{
  "status": "ok",
  "merged": [
    {
      "link": "http://startrek.myblog.it/list/3_stelle_la_qualita_comincia_a_migliorare_/star-trek-phoenix.html",
      "rank_diff": 2,
      "doc": "clueweb12-0006wb-55-07752",
      "title": "<em>Star</em> <em>Trek</em> Phoenix : <em>STAR</em> <em>TREK</em>",
      "snippet": "....."
    },
    ...
  ],
  "topk": 3,
  "baseline": [
    "clueweb12-1204wb-68-01670",
    "clueweb12-1408wb-92-06528",
    "clueweb12-0006wb-55-07752"
  ],
  "time": 5.73362421290949,
  "query": "star trek"
}
</pre>

        <h3>Literature</h3>
        <ul>
            <li>First Prototype and Ideas: <a href="http://www.uni-weimar.de/medien/webis/publications/papers/stein_2014p.pdf" >Webis at TREC 2014: Web, Session, and Contextual Suggestion Tracks</a> </li>
            <li>More Axioms and better weight lerarning: <a href="to-appear" >WSDM Paper</a> </li>

        </ul>


        </article>
        </div>

    </section>
    </div>
    <?php
      include_once('footer.php');
    ?>

  </body>

</html>


