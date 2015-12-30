<?php
/*
    This file is part of PhrasIt.

    PhrasIt is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    PhrasIt is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with PhrasIt.  If not, see <http://www.gnu.org/licenses/>.
*/
?>
    <?php
    if (!ini_get('date.timezone')) {
        date_default_timezone_set('Europe/Berlin');
    }
    ?>
    <footer>
      &copy; stg7 <?php  echo date("Y"); ?> - <a href="https://github.com/stg7" >Steve Göring</a>
    </footer>
