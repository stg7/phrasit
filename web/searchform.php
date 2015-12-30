<form class="form-inline search" method="get" action="./results.php" >
    <input name="query" type="query" class="form-control"
        placeholder="Search for Phrases" style="width:40%" required>

    <div class="btn-group">
        <button type="submit" class="btn btn-success go"><span class="glyphicon glyphicon-search"></span></button>
        <button type="button" class="btn btn-success dropdown-toggle" data-toggle="dropdown" aria-haspopup="true" aria-expanded="false">
            <span class="caret"></span>
            <span class="sr-only">Toggle Dropdown</span>
        </button>
        <ul class="dropdown-menu">
            <li>matching one word</li>
            <li><a href="#" class="example-query">hello ?</a></li>
            <li role="separator" class="divider"></li>
            <li>matching 0.. words</li>
            <li><a href="#" class="example-query">hello *</a></li>
            <li role="separator" class="divider"></li>
            <li>check which word fits as best</li>
            <li><a href="#" class="example-query">the same [ like as ]</a></li>
            <li role="separator" class="divider"></li>
            <li>check ordering of words</li>
            <li><a href="#" class="example-query">{ only for members }</a></li>
        </ul>
    </div>
</form>


