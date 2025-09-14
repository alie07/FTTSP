@rem = '--*-Perl-*--
@echo off
perl -x -S %0 %1 %2 %3 %4 %5 %6 %7 %8 %9
goto endofperl
@rem ';
#!perl -w
#line 8

#
# This script renames a project by renaming some key files and
# modifying the make file.
#
# $Author: troth $
# $Date: 1999/04/02 01:15:04 $
#
# $Revision: 1.1 $
#

# Revision History:
#
# 08/06/01  001.00  Steven     Modify for VC++ 6.0
#

use strict;
use Win32;

sub DOSPath {
	my( $path ) = @_;
	return $path if ! defined( $path );
	$path =~ s#/#\\#g;
	$path =~ s#\\\\#\\#g;
	return $path;
}

sub Die {
	my( $message ) = @_;
	printf( STDOUT "%s\n", $message );
	printf( STDOUT "Hit return to exit..." );
	scalar( <STDIN> );
	die;
}

sub GoodBye {
	#
	# Say goodbye
	#
	my ($Msg) = @_;

	print( "\n$Msg\n" );
	print( "Hit return to exit. " );
	scalar( <STDIN> );
}

sub rename_proj {

	print ("Working, please wait! \n\n");
	my @path = split /\\/, Win32::GetCwd();
	my $oldfolder = $path[$#path];
	my $oldproj = $oldfolder;
	my @project = <*.dsw>;
	if ( @project != 1 ) { Die( "No or Too many .dsw files" ); }	

	my @tempproj = split( /\./, $project[0]);
	$oldproj = $tempproj[0];
	#my $oldproj = (fileparse( $project[0], '\..*' ))[0];

	print( "\nThe script provides a simple way to rename a project.\n");
	print( "If you ran this by accident, then either hit 'enter' twice,\n");
	print( "or click the X box in the top right corner of the window.\n");

	print( "\nOld project name: $oldproj. Enter new project name: " );
	my $newproj = <STDIN>;
	chop $newproj;
	if ("$newproj" eq "") {
		Die("\nProject unmodified.\n");
	}

	if ("$newproj" eq "$oldproj") {
		Die("\nI refuse to change $oldproj to $newproj. Sorry.\n");
	}

	#
	# As usual, NT won't let us rename a directory while we are in it. Bummer!
	#
	# print( "\nOld folder name: $oldfolder. Enter new folder name [${newproj}_000_0]: " );
	# my $newfolder = <STDIN>;
	# chop $newfolder;
	# if ("$newfolder" eq "") {
	# 	$newfolder = "${newproj}_000_0";
	# }
	#
	# print("  renaming $oldfolder folder to $newfolder\n");
	# chdir "..";
	# rename( $oldfolder, $newfolder ) or Die( "rename failed: $!" );
	# chdir( "$newfolder" );

	print "$oldproj \n";

	if ( -f "$oldproj.ncb" ) {
		#print("  unlink $oldproj.ncb\n");
		unlink( "$oldproj.ncb" ) or Die("Can't delete $oldproj.ncb: $!");
	}

	if ( -f "$oldproj.plg" ) {
		#print("  unlink $oldproj.plg\n");
		unlink( "$oldproj.plg" ) or Die("Can't delete $oldproj.plg: $!");
	}

	if ( -f "$oldproj.opt" ) {
		#print("  unlink $oldproj.opt\n");
		unlink( "$oldproj.opt" ) or Die("Can't delete $oldproj.opt: $!");
	}

	if ( -f "$oldproj.mak" ){
		print("  renaming: $oldproj.mak -> $newproj.mak\n");
		open(OLDMAK,"$oldproj.mak") || Die("Can't open $oldproj.mak: $!");
		open(NEWMAK,">$newproj.mak") || Die("Can't open $newproj.mak: $!");
		while ( <OLDMAK> ) {
			my $line;
			#($line = $_) =~ s/$oldproj/$newproj;
			($line = $_) =~ s/["\\= ]$oldproj([ "])/\"$newproj$1/ig;
			$_ = $line;
			($line = $_) =~ s/$oldproj\./$newproj\./ig;
			print NEWMAK $line;
		}
		close(OLDMAK);
		close(NEWMAK);
		unlink( "$oldproj.mak" ) or Die("Can't delete $oldproj.mak: $!");
	}

	if ( -f "$oldproj.dsw" ){
		print("  renaming: $oldproj.dsw -> $newproj.dsw\n");
		open(OLDDSW,"$oldproj.dsw") || Die("Can't open $oldproj.dsw: $!");
		open(NEWDSW,">$newproj.dsw") || Die("Can't open $newproj.dsw: $!");
		while ( <OLDDSW> ) {
			my $line;
			#($line = $_) =~ s/$oldproj/$newproj;
			($line = $_) =~ s/["\\= ]$oldproj([ "])/\"$newproj$1/ig;
			$_ = $line;
			($line = $_) =~ s/$oldproj\./$newproj\./ig;
			print NEWDSW $line;
		}
		close(OLDDSW);
		close(NEWDSW);
		unlink( "$oldproj.dsw" ) or Die("Can't delete $oldproj.dsw: $!");
	}

	if ( -f "$oldproj.dsp" ){
		print("  renaming: $oldproj.dsp -> $newproj.dsp\n");
		open(OLDDSP,"$oldproj.dsp") || Die("Can't open $oldproj.dsp: $!");
		open(NEWDSP,">$newproj.dsp") || Die("Can't open $newproj.dsp: $!");
		while ( <OLDDSP> ) {
			my $line;
			#($line = $_) =~ s/$oldproj/$newproj;
			($line = $_) =~ s/["\\= ]$oldproj([ "])/\"$newproj$1/ig;
			$_ = $line;
			($line = $_) =~ s/$oldproj(.dsw)/$newproj$1/ig;
			$_ = $line;
			($line = $_) =~ s/$oldproj(.dsp)/$newproj$1/ig;
			$_ = $line;
			($line = $_) =~ s/$oldproj(.mak)/$newproj$1/ig;
			print NEWDSP $line;
		}
		close(OLDDSP);
		close(NEWDSP);
		unlink( "$oldproj.dsp" ) or Die("Can't delete $oldproj.dsp: $!");
	}

	
	if ( -f "$oldproj.mdp" ){
	    print("  renaming: ${oldproj}.mdp -> ${newproj}.mdp\n");
	    rename( "${oldproj}.mdp", "${newproj}.mdp" ) or Die( "rename failed: $!" );
	}

	if ( -f "$oldproj.001" ){
		print("  renaming: $oldproj.mak -> $newproj.mak\n");
		open(OLDMAK,"$oldproj.001") || Die("Can't open $oldproj.001: $!");
		open(NEWMAK,">$newproj.001") || Die("Can't open $newproj.001: $!");
		while ( <OLDMAK> ) {
			my $line;
			#($line = $_) =~ s/$oldproj/$newproj;
			($line = $_) =~ s/["\\= ]$oldproj([ "])/\"$newproj$1/ig;
			$_ = $line;
			($line = $_) =~ s/$oldproj\./$newproj\./ig;
			print NEWMAK $line;
		}
		close(OLDMAK);
		close(NEWMAK);
		unlink( "$oldproj.001" ) or Die("Can't delete $oldproj.001: $!");
	}

	#
	# directories which may contain stuff we don't 
	# need to worry about renaming.
	#
	if (-d "Debug") {	
		#print( "  Deleting Debug folder\n" );
		system("rmdir /S /Q Debug");
	}
	
	if (-d "Release") {
		#print( "  Deleting Release folder\n" );
		system ("rmdir /S /Q Release");
	}

	print( "\n********************************************************\n" );
	print( "\nYOU must now change the name of the current folder.\n" );
	print( "\nDon't for get to update the mav_rev.cpp file.\n" );
	print( "\n********************************************************\n" );

}


rename_proj( );
GoodBye( "All done" );

1;	# sometime this file is require( )ed


__END__
:endofperl
