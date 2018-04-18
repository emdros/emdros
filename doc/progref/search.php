<?php

function parse_http_get_post_vars() 
{
  global $arr_request;
  global $HTTP_GET_VARS;
  global $HTTP_POST_VARS;

  // declare the request array which holds both
  // url-based (get) and form-based (post) parameters. 
  $arr_request = array();

  // move the url and form parameters into the 
  // request array. Form parameters supercede url
  // parameters. Additionally, all keys are converted
  // to lower-case.
  if (count($HTTP_GET_VARS)) {
    while (list($key, $value) = each ($HTTP_GET_VARS)) {
      $arr_request[strtolower($key)] = $value;
    }
  }
  if (count($HTTP_POST_VARS)) {
    while (list($key, $value) = each ($HTTP_POST_VARS)) {
      $arr_request[strtolower($key)] = $value;
    }
  }
}


function get_link_from_id($str_doc_id) {
  global $arr_files;
  global $arr_request;
  $str_NodeType = $arr_files[$str_doc_id]["NodeType"];
  if ($arr_request["static_links"] == "true") {
    // Page with static links
    switch ($str_NodeType) {
    case "Indhold":
    case "P":
    case "Part":
    case "C":
    case "E":
    case "Es":
    case "Hint":
    case "Answer":
    case "A":
      $str_result = "" . $str_doc_id . ".html";
      break;
    default:
      echo "<STRONG>Unknown nodetype '$str_NodeType' in document $str_doc_id.</STRONG>";
      exit;
    }
  } else {
    // Normal page
    switch ($str_NodeType) {
    case "Indhold":
    case "P":
    case "Part":
    case "C":
    case "E":
    case "Es":
    case "Hint":
    case "Answer":
    case "A":
      $str_result = "page.php?pid=" . $str_doc_id;
      break;
    default:
      echo "<STRONG>Unknown nodetype '$str_NodeType' in document $str_doc_id.</STRONG>";
      exit;
    }
  }
  return $str_result;
}


function get_decorated_doc_title($str_doc_id) {
  global $arr_files;
  // $str_decoration = get_doc_decoration($str_doc_id);
  $str_doc_title = $arr_files[$str_doc_id][Title];
/*
  if ($str_decoration != "")
    return $str_decoration . "&nbsp;" . $str_doc_title;
  else 
    return $str_doc_title;
*/
  return $str_doc_title;
} 


function print_search_header() {
  global $str_site_title;
  echo "<HTML>\n";
  echo "  <HEAD>\n";
  echo "    <TITLE>" . $str_site_title . ": Search results</TITLE>\n";
  echo "    <LINK HREF=\"progref.css\" rel=\"stylesheet\" type=\"text/css\">\n";
  echo "    <LINK rel=\"shortcut ico\" HREF=\"http://emdros.org/favicon.ico\">";
  echo "    <BASE target=\"_top\">\n";
  echo "    <meta http-equiv=\"Content-Type\" content=\"text/html; charset=iso-8859-1\">\n";
  echo "    <META http-equiv=\"Content-Style-Type\" content=\"text/css\">\n";
  echo "  </HEAD>\n";
  echo "<BODY bgcolor=\"#FFFFFF\">\n";
  echo "<H1>Search results</H1>\n";
}

function print_search_footer() {
  echo "<P></P><HR>\n";
  echo "</BODY>\n";
  echo "</HTML>\n";
}

function print_new_search()
{
  global $terms;
  
  echo "<h1>New search</h1>\n";
  echo "<FORM ACTION=\"search.php\" METHOD=\"GET\">\n";
  echo "<INPUT TYPE=\"HIDDEN\" NAME=\"static_links\" VALUE=\"true\">";
  echo "<INPUT TYPE=\"TEXT\" NAME=\"terms\" VALUE=\"$terms\" SIZE=\"50\" ALT=\"Write search terms\">\n";
  echo "<INPUT TYPE=\"SUBMIT\" VALUE=\"Search\">\n";
  echo "</FORM>\n";

}

function print_search_results(&$arr_results)
{
  if (count($arr_results) == 0) {
    echo "<em>No search results.</em>";
  } else {
    // Issue start-OL
    echo "<OL>\n";

    // Iterate through results
    reset($arr_results);
    $do_continue = 1;
    do {
      // Get current ID
      $str_page_id = current($arr_results);

      // Get title
      $str_title = get_decorated_doc_title($str_page_id);

      // Get link
      $str_link = get_link_from_id($str_page_id);

      // Print result
      echo "<LI><A HREF=\"$str_link\">$str_title</A>\n";

      // Should we continue?
      if (!next($arr_results)) {
        $do_continue = 0;
      } 
    } while ($do_continue);

    // Issue end-OL
    echo "</OL>\n";
  }
}




function read_file($str_filename) {
  global $arr_files;
  $file = fopen ($str_filename, "r");
  if (!$file) {
    echo "<p>Unable to open file $filename.</p>\n";
    exit;
  }
  while (!feof ($file)) {
    $str_line = fgets ($file, 1024);
    if ($str_line) {
      list ($str_id, $str_title, $str_next, $str_up, $str_node_type) = split("\t|\n", $str_line);
      $str_prev = "-1";
      $arr_files[$str_id] = array(
        "Title" => $str_title,
        "Prev" => "" . $str_prev,
        "Up" => "" . $str_up,
        "Next" => "" . $str_next,
        "NodeType" => "" . $str_node_type);
    }
  }
  fclose($file);
}

function calculate_prev() {
  global $arr_files;
  if (count($arr_files)) { 
    reset($arr_files);
    $do_continue = 1;
    do {
      // Get current ID
      $str_id = key($arr_files);

      // Get Next 
      $str_next = $arr_files[$str_id]["Next"];

      // Deal with next
      if ($str_next != "-1") {
        $arr_files[$str_next]["Prev"] = $str_id;
      }

      // Should we continue?
      if (!next($arr_files)) {
        $do_continue = 0;
      } 
    } while ($do_continue);
  } 
}



function init_page($str_id) {
  global $str_page_id;
  $str_page_id = $str_id;
  read_file("files.txt");
  calculate_prev();
  // enumerate_documents();
}


function parse_terms()
{
  global $terms;
  global $arr_request;
  $terms = $arr_request["terms"];

  // Trim whitespace off beginning and end
  $str_terms = trim($terms);

  // Remove "+-"
  $str_terms = preg_replace("/[-+\"]/", " ", $str_terms);

  // Tokenize string
  $arr_terms = preg_split("/[ \t\n]+/", $str_terms);

  // Make real array
  $arr_search_terms = array();

  // Iterate through $arr_terms
  reset($arr_terms);
  $do_continue = 1;
  do {
    // Get current ID
    $str_term = current($arr_terms);

    // Insert into array
    $arr_search_terms[$str_term] = "false";

    // Should we continue?
    if (!next($arr_terms)) {
      $do_continue = 0;
    } 
  } while ($do_continue);

  // Return value
  return $arr_search_terms;
}



function get_page(&$file, &$str_page)
{
  // Iterate through lines of file
  $int_do_continue = 1;
  $str_page = "";
  while (!feof ($file) && $int_do_continue) {
      $str_line = fgets ($file, 4096);
      if ($str_line) {
	  if (preg_match("/\<\/page\>/i", $str_line)) {
	    $int_do_continue = 0;
          } else {
//            $str_line = str_replace("\n", " ", $str_line);
            $str_page = $str_page . $str_line;
          }
      }
  }
}

function all_search_terms_matched(&$arr_search_terms)
{
  // Assume all matched
  $int_result = 1;

  // Iterate through search terms
  reset($arr_search_terms);
  $do_continue = 1;
  do {
    // Get current ID
    $str_was_match = current($arr_search_terms);

    // test: If any one was false, all didn't match
    if ($str_was_match == "false") {
      $int_result = 0;
      $do_continue = 0;
    }

    // Should we continue?
    if (!next($arr_search_terms)) {
      $do_continue = 0;
    } 
  } while ($do_continue);

  // Return result
  return $int_result;
}

function parse_page_searching(&$file, $arr_search_terms)
{
  // Get page
  get_page($file, $str_page);

  // Iterate through search terms
  reset($arr_search_terms);
  $do_continue = 1;
  do {
    // Get current ID
    $str_term = key($arr_search_terms);

    // Is this in page?
    if (preg_match("~$str_term~", $str_page)) {
      // This term matched
      $arr_search_terms[$str_term] = "true";
    }

    // Should we continue?
    if (!next($arr_search_terms)) {
      $do_continue = 0;
    } 
  } while ($do_continue);

  // Were they all true?
  return all_search_terms_matched($arr_search_terms);
}

// Parse file
function parse_file_searching($str_filename, $arr_search_terms, &$arr_results)
{
  // Check whether file exists
  if (!file_exists($str_filename)) {
    echo "<p>File $str_filename not found.</p>\n";
    exit;
  }

  // Open file
  $file = fopen ($str_filename, "r");

  // Check that file is open
  if (!$file) {
    echo "<p>Couldn't open file $str_filename.</p>\n";
    exit;
  }

  // Generate line to look for
  $str_pattern_to_look_for = "|<page id=\"|i";

  // Iterate through lines of file
  $int_do_continue = 1;
  while (!feof ($file) && $int_do_continue) {
      $str_line = fgets ($file, 4096);
      if ($str_line) {
	  if (preg_match($str_pattern_to_look_for, $str_line)) {
            // Get ID
            preg_match('/<page[ \t]ID=\"([0-9]+)\"/i', $str_line, $arr_matches);
            $str_id = $arr_matches[1];

            // Parse page
            if (parse_page_searching($file, $arr_search_terms)) {
              array_push($arr_results, $str_id);
            };
          }
      }
  }

  // Close file
  fclose($file);
}

// Parse files
function parse_files_searching(&$arr_search_terms, &$arr_results)
{
  parse_file_searching("Content.txt", $arr_search_terms, $arr_results);
}

function perform_search()
{
  // Parse terms
  $arr_search_terms = parse_terms();

  // Create result-array
  $arr_results = array();

  // Parse files
  parse_files_searching($arr_search_terms, $arr_results);

  // Return results
  return $arr_results;
}


function search()
{
  // Find out what our GET/POST parameters are
  parse_http_get_post_vars();

  // Init page
  init_page("5000");

  // Print header
  print_search_header();

  // perform_search
  $arr_results = perform_search();

  // Print results
  print_search_results($arr_results);

  // Print new search
  print_new_search();

  // Print footer
  print_search_footer();
}

  search();
?>