#! /bin/sh -e

# $1 - TRAVIS_BUILD_NUMBER
# $2 - BINTRAY_API_KEY

baseurl='https://api.bintray.com/content'
project='bulkmt'
name='bulkmt'
version="0.0.$1"
user='edwdmkrv'
params=';deb_distribution=xenial;deb_component=main;deb_architecture=amd64;publish=1'

package="${name}_${version}_amd64.deb"

curl -T "$package" -u "${user}:$2" "$baseurl/$user/$project/$name/$version/pool/main/${name::1}/$name/$package$params"
