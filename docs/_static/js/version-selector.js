!function() {
    //  <root_url>/<language>/<repo>/<version>
    let currentVersion = DOCUMENTATION_OPTIONS.VERSION;
    let language = DOCUMENTATION_OPTIONS.LANGUAGE;
    let currentRepoTarget = USER_DEFINED_OPTIONS.REPO_PATH;
    let rootUrlPrefix = USER_DEFINED_OPTIONS.URL_ROOT_PREFIX;
    let urlRoot = rootUrlPrefix ? "/" + rootUrlPrefix + "/" : "/"
    // 语言切换预留占位
    let languagePath = language ? "/" + language + "/" : "/"
    function toVersionHref(event) {
        let version = event.target.value;
        let hrefUrl = urlRoot + currentRepoTarget + languagePath + version;
        console.log(hrefUrl)
        window.location.href = hrefUrl;
    };
    var versionSelect = document.getElementById("version-select");
    const versionSelector = document.querySelector('.version-selector')
    const versionOptions = document.getElementById('version-options')
    versionSelector.addEventListener('click', () => {
        if (versionOptions.style.display == 'none' || !versionOptions.style.display) {
            versionOptions.style.display = 'block'
            versionSelector.classList.add('active')
            versionSelect.focus()
        } else {
            versionOptions.style.display = 'none'
            versionSelector.classList.remove('active')
        }
    })
    versionSelect.addEventListener('blur', () => {
        setTimeout(() => {
            versionOptions.style.display = 'none'
            versionSelector.classList.remove('active')
        }, 100);
    })
    versionSelect.hidden = false;
    versionSelect.onchange = toVersionHref;
    versionOptions.addEventListener('click', (e) => {
        if (e.target.className === 'option') {
            versionSelect.value = e.target.innerText
            versionSelector.setAttribute('title', versionSelect.value)
            $('.version-readOnly').text(versionSelect.value)
            var event = new Event('change');
            versionSelect.dispatchEvent(event);
            versionOptions.style.display = 'none'
        }
    })
    let release_versions = DOCUMENTATION_VERSIONS.VERSIONS.release;

    for (let i = 0; i < release_versions.length; i++) {
        let version = release_versions[i];
        if (version.enable) {
            const option = document.createElement('li')
            option.setAttribute('class', 'option')
            option.setAttribute('title', version.name)
            option.innerText = version.name
            versionOptions.append(option);
        };
    };
    let pre_release_versions = DOCUMENTATION_VERSIONS.VERSIONS.pre_release;
    for (let i = 0; i < pre_release_versions.length; i++) {
        let version = pre_release_versions[i];
        if (version.enable) {
            const option = document.createElement('li')
            option.setAttribute('class', 'option')
            option.setAttribute('title', version.name)
            option.innerText = version.name
            versionOptions.append(option);
        };
    };
    versionSelect.value = currentVersion;
    versionSelector.setAttribute('title', versionSelect.value)
    $('.version-readOnly').text(versionSelect.value)
    function toRepoHref(event) {
        let repo = event.target.value;
        let hrefUrl = urlRoot + repo + languagePath + currentVersion;
        console.log(hrefUrl);
        window.location.href = hrefUrl;
    };
    var repoSelect = document.getElementById("repo-select");
    const repoSelector = document.querySelector('.repo-selector')
    const repoOptions = document.getElementById('repo-options')
    repoSelector.addEventListener('click', () => {
        if (repoOptions.style.display == 'none' || !repoOptions.style.display) {
            repoOptions.style.display = 'block'
            repoSelector.classList.add('active')
            repoSelect.focus()
        } else {
            repoOptions.style.display = 'none'
            repoSelector.classList.remove('active')
        }
    })
    repoSelect.addEventListener('blur', () => {
        setTimeout(() => {
            repoOptions.style.display = 'none'
            repoSelector.classList.remove('active')
        }, 100);
    })
    repoOptions.addEventListener('click', (e) => {
        if (e.target.className === 'option') {
            repoSelect.value = e.target.innerText
            repoSelector.setAttribute('title', repoSelect.value)
            $('.repo-readOnly').text(repoSelect.value)
            var event = new Event('change');
            repoSelect.dispatchEvent(event);
            repoOptions.style.display = 'none'
        }
    })
    repoSelect.hidden = false;
    repoSelect.onchange = toRepoHref;
    let repoTargets = DOCUMENTATION_VERSIONS.REPO_TARGETS
    repoSelect.value = currentRepoTarget;
    repoSelector.setAttribute('title', repoSelect.value)
    $('.repo-readOnly').text(repoSelect.value)
    if (repoTargets.length === 0) {
        $('.repo-select').hide()
        $('.version-select').addClass('only-version')
    }

    for (let i = 0; i < repoTargets.length; i++) {
        let repoTarget = repoTargets[i];
        const option = document.createElement('li')
        option.setAttribute('class', 'option')
        option.setAttribute('title', repoTarget.value)
        option.innerText = repoTarget.value
        repoOptions.append(option)
    }
    window.onpopstate = function() {
        if (location.href.match(/\#$/)) {
            $('.wy-nav-content').scrollTop(0)
        }
    }

}()